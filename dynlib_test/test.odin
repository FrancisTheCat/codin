package test

@(export)
hello: string = "Hellope from Odin!"

@(export)
four: i32 = add(1, 2)

@(export)
add :: proc "c" (a, b: i32) -> i32 {
	return a + b
}

@(export)
say_hello :: proc "c" () -> string {
	hello = "Goodbye from Odin"
	return "Hello!!!"
}
