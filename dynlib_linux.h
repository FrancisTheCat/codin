#include "codin.h"

b8 _dynlib_load(String path, Allocator allocator, Dynlib *lib) {
  Fd fd = or_do_err(file_open(path, FP_Read), _err, {
    return false;
  });
  File_Info info;
  OS_Error err = file_stat(fd, &info);
  if (err) {
    file_close(fd);
    return false;
  }
  lib->mapping = (rawptr)syscall(SYS_mmap, nil, info.size, PROT_READ, MAP_PRIVATE | MAP_FILE, fd, 0);
  if (!lib->mapping) {
    file_close(fd);
    return false;
  }

  typedef struct {
    char    e_ident[16];
    u16     e_type;
    u16     e_machine;
    u32     e_version;
    rawptr  e_entry;
    uintptr e_phoff;
    uintptr e_shoff;
    u32     e_flags;
    u16     e_ehsize;
    u16     e_phentsize;
    u16     e_phnum;
    u16     e_shentsize;
    u16     e_shnum;
    u16     e_shstrndx;
  } Elf64_Ehdr;

  typedef struct {
    u32     sh_name;
    u32     sh_type;
    u64     sh_flags;
    uintptr sh_addr;
    uintptr sh_offset;
    u64     sh_size;
    u32     sh_link;
    u32     sh_info;
    u64     sh_addralign;
    u64     sh_entsize;
  } Elf64_Shdr;

  if (info.size < size_of(Elf64_Ehdr)) {
    file_close(fd);
    return false;
  }
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)lib->mapping;
  if (!string_equal((String){.data = (char const *)ehdr->e_ident, .len = 4}, LIT("\x7F" "ELF"))) {
    file_close(fd);
    return false;
  }
  if (ehdr->e_type != 3) {
    file_close(fd);
    return false;
  }

  typedef enum {
    SHT_NULL	         = 0,
    SHT_PROGBITS	     = 1,
    SHT_SYMTAB	       = 2,
    SHT_STRTAB	       = 3,
    SHT_RELA	         = 4,
    SHT_HASH	         = 5,
    SHT_DYNAMIC	       = 6,
    SHT_NOTE	         = 7,
    SHT_NOBITS	       = 8,
    SHT_REL		         = 9,
    SHT_SHLIB	         = 10,
    SHT_DYNSYM	       = 11,
    SHT_INIT_ARRAY	   = 14,
    SHT_FINI_ARRAY	   = 15,
    SHT_PREINIT_ARRAY  = 16,
    SHT_GROUP	         = 17,
    SHT_SYMTAB_SHNDX   = 18,
    SHT_RELR	         = 19,
    SHT_NUM		         = 20,
    SHT_LOOS	         = 0x60000000,
    SHT_GNU_ATTRIBUTES = 0x6ffffff5,
    SHT_GNU_HASH	     = 0x6ffffff6,
    SHT_GNU_LIBLIST	   = 0x6ffffff7,
    SHT_CHECKSUM	     = 0x6ffffff8,
    SHT_LOSUNW	       = 0x6ffffffa,
    SHT_SUNW_move	     = 0x6ffffffa,
    SHT_SUNW_COMDAT    = 0x6ffffffb,
    SHT_SUNW_syminfo   = 0x6ffffffc,
    SHT_GNU_verdef	   = 0x6ffffffd,
    SHT_GNU_verneed	   = 0x6ffffffe,
    SHT_GNU_versym	   = 0x6fffffff,
    SHT_HISUNW	       = 0x6fffffff,
    SHT_HIOS	         = 0x6fffffff,
    SHT_LOPROC	       = 0x70000000,
    SHT_HIPROC	       = 0x7fffffff,
    SHT_LOUSER	       = 0x80000000,
    SHT_HIUSER	       = 0x8fffffff,
  } Section_Header_Type;

  typedef struct {
    u32     st_name;
    u8      st_info;
    u8      st_other;
    u16     st_shndx;
    uintptr st_value;
    u64     st_size;
  } Elf64_Sym;

  typedef struct {
    uintptr r_offset;
    u64     r_info;
  } Elf64_Rel;

  typedef struct {
    uintptr r_offset;
    u32     r_type;
    u32     r_sym;
    i64     r_addend;
  } Elf64_Rela;

  Byte_Slice string_table         = {0};
  Byte_Slice section_string_table = {0};

  Vector(Elf64_Sym) symbols;
  vector_init(&symbols, 0, 8, context.temp_allocator);

  Vector(Elf64_Rela) relas;
  vector_init(&relas, 0, 8, context.temp_allocator);

  Vector(Elf64_Rel) rels;
  vector_init(&rels, 0, 8, context.temp_allocator);

  typedef struct {
    isize   name;
    uintptr offset;
    uintptr address;
    uintptr size;
    u64     flags;
  } Section;

  Vector(Section) sections;
  vector_init(&sections, 0, 8, context.temp_allocator);

  for_range(i, 0, ehdr->e_shnum) {
    Elf64_Shdr *shdr = (Elf64_Shdr *)((uintptr)lib->mapping + ehdr->e_shoff + ehdr->e_shentsize * i);

    Section section = (Section) {
      .name    = shdr->sh_name,
      .offset  = shdr->sh_offset,
      .address = shdr->sh_addr,
      .size    = shdr->sh_size,
      .flags   = shdr->sh_flags,
    };
    vector_append(&sections, section);

    switch (shdr->sh_type) {
    case SHT_NULL:
      break;
    case SHT_PROGBITS:
      break;
    case SHT_SYMTAB:
      break;
    case SHT_STRTAB:
      if (string_table.len) {
        section_string_table = (Byte_Slice) {
          .data = (byte *)((uintptr)lib->mapping + shdr->sh_offset),
          .len  = (isize)shdr->sh_size,
        };
      } else {
        string_table = (Byte_Slice) {
          .data = (byte *)((uintptr)lib->mapping + shdr->sh_offset),
          .len  = (isize)shdr->sh_size,
        };
      }

      // base  = (char *)((uintptr)lib->mapping + shdr->sh_offset);
      // chunk = (Byte_Slice) {.data = (byte *)base, .len = (isize)shdr->sh_size};
      // vector_append_slice(&string_table, chunk);
      break;
    case SHT_RELA:
      for_range(j, 0, shdr->sh_size / shdr->sh_entsize) {
        Elf64_Rela *rela = (Elf64_Rela *)((uintptr)lib->mapping + shdr->sh_offset + shdr->sh_entsize * j);
        vector_append(&relas, *rela);
      }
      break;
    case SHT_HASH:
      break;
    case SHT_DYNAMIC:
      break;
    case SHT_NOTE:
      break;
    case SHT_NOBITS:
      break;
    case SHT_REL:
      for_range(j, 0, shdr->sh_size / shdr->sh_entsize) {
        Elf64_Rel *rel = (Elf64_Rel *)((uintptr)lib->mapping + shdr->sh_offset + shdr->sh_entsize * j);
        vector_append(&rels, *rel);
      }
      break;
    case SHT_SHLIB:
      break;
    case SHT_DYNSYM:
      for_range(j, 0, shdr->sh_size / shdr->sh_entsize) {
        Elf64_Sym *sym = (Elf64_Sym *)((uintptr)lib->mapping + shdr->sh_offset + shdr->sh_entsize * j);
        vector_append(&symbols, *sym);
      }
      break;
    case SHT_INIT_ARRAY:
      break;
    case SHT_FINI_ARRAY:
      break;
    case SHT_PREINIT_ARRAY:
      break;
    case SHT_GROUP:
      break;
    case SHT_SYMTAB_SHNDX:
      break;
    case SHT_RELR:
      break;
    case SHT_NUM:
      break;
    case SHT_LOOS:
      break;
    case SHT_GNU_ATTRIBUTES:
      break;
    case SHT_GNU_HASH:
      break;
    case SHT_GNU_LIBLIST:
      break;
    case SHT_CHECKSUM:
      break;
    case SHT_LOSUNW:
      break;
    case SHT_SUNW_COMDAT:
      break;
    case SHT_SUNW_syminfo:
      break;
    case SHT_GNU_verdef:
      break;
    case SHT_GNU_verneed:
      break;
    case SHT_GNU_versym:
      break;
    case SHT_LOPROC:
      break;
    case SHT_HIPROC:
      break;
    case SHT_LOUSER:
      break;
    case SHT_HIUSER:
      break;
    }
  }

  typedef enum {
    SHF_WRITE	           = (1 << 0),	/* Writable */
    SHF_ALLOC	           = (1 << 1),	/* Occupies memory during execution */
    SHF_EXECINSTR        = (1 << 2),	/* Executable */
    SHF_MERGE	           = (1 << 4),	/* Might be merged */
    SHF_STRINGS	         = (1 << 5),	/* Contains nul-terminated strings */
    SHF_INFO_LINK	       = (1 << 6),	/* `sh_info' contains SHT index */
    SHF_LINK_ORDER	     = (1 << 7),	/* Preserve order after combining */
    SHF_OS_NONCONFORMING = (1 << 8),	/* Non-standard OS specific handling required */
    SHF_GROUP	           = (1 << 9),	/* Section is member of a group.  */
    SHF_TLS		           = (1 << 10),	/* Section hold thread-local data.  */
    SHF_COMPRESSED	     = (1 << 11),	/* Section with compressed data. */
    SHF_GNU_RETAIN	     = (1 << 21),  /* Not to be GCed by linker.  */
    SHF_ORDERED	         = (1 << 30),	/* Special ordering requirement (Solaris).  */
    SHF_EXCLUDE	         = (1U << 31),	/* Section is excluded unless referenced or allocated (Solaris).*/
  } Section_Header_Flag;

  uintptr mapping_size = 0;

  vector_iter(sections, section, i, {
    mapping_size = max(mapping_size, !!(section->flags & SHF_ALLOC) * (section->address + section->size));

    fmt_printf(
      LIT("Index: %2x, Offset: %4x, Addr: %4x, Size: %4x, Flags: %03b, Name: '%s', flags: ["),
      i,
      section->offset,
      section->address,
      section->size,
      section->flags & 0x7,
      &section_string_table.data[section->name]
    );

    if (section->flags & SHF_WRITE) {
      fmt_print(LIT("'SHF_WRITE', "));
    }
    if (section->flags & SHF_ALLOC) {
      fmt_print(LIT("'SHF_ALLOC', "));
    }
    if (section->flags & SHF_EXECINSTR) {
      fmt_print(LIT("'SHF_EXECINSTR', "));
    }
    if (section->flags & SHF_MERGE) {
      fmt_print(LIT("'SHF_MERGE', "));
    }
    if (section->flags & SHF_STRINGS) {
      fmt_print(LIT("'SHF_STRINGS', "));
    }
    if (section->flags & SHF_INFO_LINK) {
      fmt_print(LIT("'SHF_INFO_LINK', "));
    }
    if (section->flags & SHF_LINK_ORDER) {
      fmt_print(LIT("'SHF_LINK_ORDER', "));
    }
    if (section->flags & SHF_OS_NONCONFORMING) {
      fmt_print(LIT("'SHF_OS_NONCONFORMING', "));
    }
    if (section->flags & SHF_GROUP) {
      fmt_print(LIT("'SHF_GROUP', "));
    }
    if (section->flags & SHF_TLS) {
      fmt_print(LIT("'SHF_TLS', "));
    }
    if (section->flags & SHF_COMPRESSED) {
      fmt_print(LIT("'SHF_COMPRESSED', "));
    }
    if (section->flags & SHF_GNU_RETAIN) {
      fmt_print(LIT("'SHF_GNU_RETAIN', "));
    }
    if (section->flags & SHF_EXCLUDE) {
      fmt_print(LIT("'SHF_EXCLUDE', "));
    }
    fmt_println(LIT("]"));
  });

  log_infof(LIT("Required mapping: %x"), mapping_size);
  lib->mapping_size = mapping_size;

  Slice(struct {
    isize prot;
    isize offset;
  }) pages;
  slice_init(&pages, (mapping_size + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE, context.temp_allocator);

  rawptr new_mapping = (rawptr)syscall(SYS_mmap, nil, mapping_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  slice_iter(sections, section, _section_index, {
    if (section->flags & SHF_ALLOC) {
      uintptr start = section->address & ~(OS_PAGE_SIZE-1);
      uintptr end   = section->address + section->offset;
      isize   prot  = PROT_READ;

      if (section->flags & SHF_WRITE) {
        prot |= PROT_WRITE;
      }
      if (section->flags & SHF_EXECINSTR) {
        prot |= PROT_EXEC;
      }

      for_range(i, start / OS_PAGE_SIZE, (end + OS_PAGE_SIZE - 1) / OS_PAGE_SIZE) {
        pages.data[i].prot   |= prot;
        pages.data[i].offset  = section->address - section->offset;
      }

      bytes_copy(
        (Byte_Slice) {
          .data = (byte *)((uintptr)new_mapping + section->address),
          .len  = (isize)section->size,
        },
        (Byte_Slice) {
          .data = (byte *)((uintptr)lib->mapping + section->offset),
          .len  = (isize)section->size,
        }
      );
    }
  })

  lib->mapping = new_mapping;

  file_close(fd);

  fmt_printfln(LIT("relas.len: %d"), relas.len);
  for (isize _i = 0; _i < (relas).len; _i++) {
    Elf64_Rela *rela = &(relas).data[_i];
    fmt_printfln(
      LIT("\tRela: {offset: %x, sym: %x, type: %x, addend: %x}"),
      rela->r_offset,
      (isize)rela->r_sym,
      (isize)rela->r_type,
      rela->r_addend
    );

    typedef enum {
      R_X86_64_NONE            = 0,  /* No reloc */
      R_X86_64_64              = 1,  /* Direct 64 bit  */
      R_X86_64_PC32            = 2,  /* PC relative 32 bit signed */
      R_X86_64_GOT32           = 3,  /* 32 bit GOT entry */
      R_X86_64_PLT32           = 4,  /* 32 bit PLT address */
      R_X86_64_COPY            = 5,  /* Copy symbol at runtime */
      R_X86_64_GLOB_DAT        = 6,  /* Create GOT entry */
      R_X86_64_JUMP_SLOT       = 7,  /* Create PLT entry */
      R_X86_64_RELATIVE        = 8,  /* Adjust by program base */
      R_X86_64_GOTPCREL        = 9,  /* 32 bit signed PC relative offset to GOT */
      R_X86_64_32              = 10, /* Direct 32 bit zero extended */
      R_X86_64_32S             = 11, /* Direct 32 bit sign extended */
      R_X86_64_16              = 12, /* Direct 16 bit zero extended */
      R_X86_64_PC16            = 13, /* 16 bit sign extended pc relative */
      R_X86_64_8               = 14, /* Direct 8 bit sign extended  */
      R_X86_64_PC8             = 15, /* 8 bit sign extended pc relative */
      R_X86_64_DTPMOD64        = 16, /* ID of module containing symbol */
      R_X86_64_DTPOFF64        = 17, /* Offset in module's TLS block */
      R_X86_64_TPOFF64         = 18, /* Offset in initial TLS block */
      R_X86_64_TLSGD           = 19, /* 32 bit signed PC relative offset to two GOT entries for GD symbol */
      R_X86_64_TLSLD           = 20, /* 32 bit signed PC relative offset to two GOT entries for LD symbol */
      R_X86_64_DTPOFF32        = 21, /* Offset in TLS block */
      R_X86_64_GOTTPOFF        = 22, /* 32 bit signed PC relative offset to GOT entry for IE symbol */
      R_X86_64_TPOFF32         = 23, /* Offset in initial TLS block */
      R_X86_64_PC64            = 24, /* PC relative 64 bit */
      R_X86_64_GOTOFF64        = 25, /* 64 bit offset to GOT */
      R_X86_64_GOTPC32         = 26, /* 32 bit signed pc relative offset to GOT */
      R_X86_64_GOT64           = 27, /* 64-bit GOT entry offset */
      R_X86_64_GOTPCREL64      = 28, /* 64-bit PC relative offset to GOT entry */
      R_X86_64_GOTPC64         = 29, /* 64-bit PC relative offset to GOT */
      R_X86_64_GOTPLT64        = 30, /* like GOT64 says PLT entry needed */
      R_X86_64_PLTOFF64        = 31, /* 64-bit GOT relative offset to PLT entry */
      R_X86_64_SIZE32          = 32, /* Size of symbol plus 32-bit addend */
      R_X86_64_SIZE64          = 33, /* Size of symbol plus 64-bit addend */
      R_X86_64_GOTPC32_TLSDESC = 34, /* GOT offset for TLS descriptor.  */
      R_X86_64_TLSDESC_CALL    = 35, /* Marker for call through TLS descriptor.  */
      R_X86_64_TLSDESC         = 36, /* TLS descriptor.  */
      R_X86_64_IRELATIVE       = 37, /* Adjust indirectly by program base */
      R_X86_64_RELATIVE64      = 38, /* 64-bit adjust by program base */
      R_X86_64_GOTPCRELX       = 41, /* Load from 32 bit signed pc relative offset to GOT entry without REX prefix relaxable,.  */
      R_X86_64_REX_GOTPCRELX   = 42, /* Load from 32 bit signed pc relative offset to, GOT entry with REX prefix relaxable.  */
      R_X86_64_NUM             = 43,

      /* x86-64 sh_type values.  */
      SHT_X86_64_UNWIND        = 0x70000001, /* Unwind information.  */

      /* x86-64 d_tag values.  */
      #define DT_LOPROC	0x70000000	/* Start of processor-specific */
      DT_X86_64_PLT            = (DT_LOPROC + 0),
      DT_X86_64_PLTSZ          = (DT_LOPROC + 1),
      DT_X86_64_PLTENT         = (DT_LOPROC + 3),
      DT_X86_64_NUM            = 4,
    } Relocation_Type_X86_64;

    switch (rela->r_type) {
    case R_X86_64_GLOB_DAT:
      *(uintptr *)((uintptr)lib->mapping + rela->r_offset) =
        (uintptr)lib->mapping + symbols.data[rela->r_sym].st_value + rela->r_addend;
      break;
    case R_X86_64_JUMP_SLOT:
      *(uintptr *)((uintptr)lib->mapping + rela->r_offset) =
        (uintptr)lib->mapping + symbols.data[rela->r_sym].st_value + rela->r_addend;
      break;
    case R_X86_64_RELATIVE:
      *(uintptr *)((uintptr)lib->mapping + rela->r_offset) = (uintptr)lib->mapping + rela->r_addend;
      break;
    case R_X86_64_32:
      *(u32 *)((uintptr)lib->mapping + rela->r_offset) = rela->r_sym + (u32)rela->r_addend;
      break;
    case R_X86_64_32S:
      *(i32 *)((uintptr)lib->mapping + rela->r_offset) = rela->r_sym + rela->r_addend;
      break;
    case R_X86_64_16:
      *(u16 *)((uintptr)lib->mapping + rela->r_offset) = rela->r_sym + (u16)rela->r_addend;
      break;
    default:
      log_warnf(LIT("Unknown relocation type: 0x%x"), rela->r_type);
      break;
    }
  }

  fmt_printfln(LIT("rels.len: %d"), rels.len);
  vector_iter(rels, rel, _i, {
    fmt_printfln(LIT("\tRel: {offset: %x, info: %x}"), rel->r_offset, rel->r_info);
  })

  hash_map_init(&lib->symbols, 1024, string_equal, string_hash, allocator);
  
  #define ELF64_ST_BIND(val)        (((unsigned char) (val)) >> 4)
  #define ELF64_ST_TYPE(val)        ((val) & 0xf)
  #define ELF64_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))

  fmt_printfln(LIT("symbols.len: %d"), symbols.len);
  vector_iter(symbols, sym, _i, {
    fmt_printfln(
      LIT("\tSym: {name: '%s', type: %x, bind: %x, value: %x, size: %x, shndx: %x}"),
      (cstring)&string_table.data[sym->st_name],
      (isize)ELF64_ST_TYPE(sym->st_info),
      (isize)ELF64_ST_BIND(sym->st_info),
      (isize)sym->st_value,
      (isize)sym->st_size,
      (isize)sym->st_shndx
    );
    if (((isize)sym->st_info & 0xf) == 2 || ((isize)sym->st_info & 0xf) == 1 || true) {
      hash_map_insert(
        &lib->symbols,
        cstring_to_string((cstring)&string_table.data[sym->st_name]),
        (rawptr)((uintptr)lib->mapping + sym->st_value)
      );
    }
  })

  return true;
}

b8 _dynlib_unload(rawptr mapping, isize size) {
  (void)mapping;
  (void)size;

  unimplemented();
  // return syscall(SYS_munmap, mapping, size) == 0;
}
