mod args;

fn main()
{
    let args: Vec<String> = std::env::args().collect();

    if args.iter().any(|a| a == "-v" || a == "--version")
    {
        unsafe
        {
            args::print_version();
        }
        std::process::exit(0);
    }
    if args.iter().any(|a| a == "-h" || a == "--help")
    {
        args::print_help();
        std::process::exit(0);
    }

    println!("Hello, world!");
}
