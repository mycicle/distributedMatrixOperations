open Core
let () = print_endline "Welcome to matrix operations"
let () = print_endline "Please input your left hand matrix"
let () = print_endline "Please input your operation"
let () = print_endline "Please input your right hand matrix"
let () = print_endline "The answer is: "

let () = for i = 1 to Array.length Sys.argv - 1 do
  Printf.printf "[%i] %s\n" i Sys.argv.(i)
done




(* let r file = In_channel.read_lines file

let contents = r Sys.argv.(1)
let() = Printf.printf "%s" contents *)