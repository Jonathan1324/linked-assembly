extern "C" {
    fn printVersion();
}

fn print_help()
{
    println!("Usage: buildtool [OPTIONS]");
    println!("  -v, --version   Show version");
    println!("  -h, --help      Show this help message");
}

fn main()
{
    let args: Vec<String> = std::env::args().collect();

    if args.iter().any(|a| a == "-v" || a == "--version")
    {
        unsafe
        {
            printVersion(); // extern C-Funktion aufrufen
        }
        std::process::exit(0);
    }
    if args.iter().any(|a| a == "-h" || a == "--help")
    {
        print_help();
        std::process::exit(0);
    }

    println!("Hello, world!");
}
