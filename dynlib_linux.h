#include "codin.h"

rawptr _dynlib_load(String path, isize *size) {
  Fd fd = or_do_err(file_open(path, FP_Read), _err, {return nil;});
  File_Info info;
  OS_Error err = file_stat(fd, &info);
  if (err) {
    return nil;
  }
  *size = info.size;
  return (rawptr)syscall(SYS_mmap, nil, info.size, PROT_EXEC | PROT_READ, MAP_PRIVATE | MAP_FILE, fd, 0);
}

b8 _dynlib_load_symbols(Dynlib *lib, Allocator allocator) {
  hash_map_init(&lib->symbols, 1024, string_equal, string_hash, allocator);
  
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
    rawptr  sh_addr;
    uintptr sh_offset;
    u64     sh_size;
    u32     sh_link;
    u32     sh_info;
    u64     sh_addralign;
    u64     sh_entsize;
  } Elf64_Shdr;

  if (lib->size < size_of(Elf64_Ehdr)) {
    return false;
  }
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)lib->mapping;
  if (!string_equal((String){.data = (char const *)ehdr->e_ident, .len = 4}, LIT("\x7F" "ELF"))) {
    return false;
  }
  if (ehdr->e_type != 3) {
    return false;
  }
  log_infof(LIT("PHOFF: %x"), ehdr->e_phoff);
  log_infof(LIT("SHOFF: %x"), ehdr->e_shoff);

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

  Byte_Buffer string_table;
  builder_init(&string_table, 0, 8, allocator);

  Vector(Elf64_Sym) symbols;
  vector_init(&symbols, 0, 8, allocator);

  char *base;
  Byte_Slice chunk;

  for_range(i, 0, ehdr->e_shnum) {
    Elf64_Shdr *shdr = (Elf64_Shdr *)((uintptr)lib->mapping + ehdr->e_shoff + ehdr->e_shentsize * i);

    switch (shdr->sh_type) {
    case SHT_NULL:
      log_info(LIT("Type: 'SHT_NULL'"));
      break;
    case SHT_PROGBITS:
      log_info(LIT("Type: 'SHT_PROGBITS'"));
      break;
    case SHT_SYMTAB:
      log_info(LIT("Type: 'SHT_SYMTAB'"));
      break;
    case SHT_STRTAB:
      log_info(LIT("Type: 'SHT_STRTAB'"));
      base  = (char *)((uintptr)lib->mapping + shdr->sh_offset);
      chunk = (Byte_Slice) {.data = (byte *)base, .len = (isize)shdr->sh_size};
      vector_append_slice(&string_table, chunk);
      break;
    case SHT_RELA:
      log_info(LIT("Type: 'SHT_RELA'"));
      break;
    case SHT_HASH:
      log_info(LIT("Type: 'SHT_HASH'"));
      break;
    case SHT_DYNAMIC:
      log_info(LIT("Type: 'SHT_DYNAMIC'"));
      break;
    case SHT_NOTE:
      log_info(LIT("Type: 'SHT_NOTE'"));
      break;
    case SHT_NOBITS:
      log_info(LIT("Type: 'SHT_NOBITS'"));
      break;
    case SHT_REL:
      log_info(LIT("Type: 'SHT_REL'"));
      break;
    case SHT_SHLIB:
      log_info(LIT("Type: 'SHT_SHLIB'"));
      break;
    case SHT_DYNSYM:
      for_range(j, 0, shdr->sh_size / shdr->sh_entsize) {
        Elf64_Sym *sym = (Elf64_Sym *)((uintptr)lib->mapping + shdr->sh_offset + shdr->sh_entsize * j);
        vector_append(&symbols, *sym);
      }
      break;
    case SHT_INIT_ARRAY:
      log_info(LIT("Type: 'SHT_INIT_ARRAY'"));
      break;
    case SHT_FINI_ARRAY:
      log_info(LIT("Type: 'SHT_FINI_ARRAY'"));
      break;
    case SHT_PREINIT_ARRAY:
      log_info(LIT("Type: 'SHT_PREINIT_ARRA'"));
      break;
    case SHT_GROUP:
      log_info(LIT("Type: 'SHT_GROUP'"));
      break;
    case SHT_SYMTAB_SHNDX:
      log_info(LIT("Type: 'SHT_SYMTAB_SHNDX'"));
      break;
    case SHT_RELR:
      log_info(LIT("Type: 'SHT_RELR'"));
      break;
    case SHT_NUM:
      log_info(LIT("Type: 'SHT_NUM'"));
      break;
    case SHT_LOOS:
      log_info(LIT("Type: 'SHT_LOOS'"));
      break;
    case SHT_GNU_ATTRIBUTES:
      log_info(LIT("Type: 'SHT_GNU_ATTRIBUT'"));
      break;
    case SHT_GNU_HASH:
      log_info(LIT("Type: 'SHT_GNU_HASH'"));
      break;
    case SHT_GNU_LIBLIST:
      log_info(LIT("Type: 'SHT_GNU_LIBLIST'"));
      break;
    case SHT_CHECKSUM:
      log_info(LIT("Type: 'SHT_CHECKSUM'"));
      break;
    case SHT_LOSUNW:
      log_info(LIT("Type: 'SHT_LOSUNW'"));
      break;
    case SHT_SUNW_COMDAT:
      log_info(LIT("Type: 'SHT_SUNW_COMDAT'"));
      break;
    case SHT_SUNW_syminfo:
      log_info(LIT("Type: 'SHT_SUNW_syminfo'"));
      break;
    case SHT_GNU_verdef:
      log_info(LIT("Type: 'SHT_GNU_verdef'"));
      break;
    case SHT_GNU_verneed:
      log_info(LIT("Type: 'SHT_GNU_verneed'"));
      break;
    case SHT_GNU_versym:
      log_info(LIT("Type: 'SHT_GNU_versym'"));
      break;
    case SHT_LOPROC:
      log_info(LIT("Type: 'SHT_LOPROC'"));
      break;
    case SHT_HIPROC:
      log_info(LIT("Type: 'SHT_HIPROC'"));
      break;
    case SHT_LOUSER:
      log_info(LIT("Type: 'SHT_LOUSER'"));
      break;
    case SHT_HIUSER:
      log_info(LIT("Type: 'SHT_HIUSER'"));
      break;
    }
  }

  vector_iter(symbols, sym, _i, {
    if (((isize)sym->st_info & 0xf) == 2) {
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
  return syscall(SYS_munmap, mapping, size) == 0;
}
