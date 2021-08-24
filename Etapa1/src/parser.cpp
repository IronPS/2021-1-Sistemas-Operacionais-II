
#include <parser.hpp>

cxxopts::ParseResult parseClient(int argc, char* argv[]) {
    cxxopts::Options options("IncrebileTvitter", "Welcome to IncredibleTviter!");

    // Add options here
    options.add_options()
        ("h,help", "Print usage")
        ("u,user", "Username with which to sign in", cxxopts::value<std::string>())
        ("s,server", "Server address (IPV4)", cxxopts::value<std::string>()->default_value("127.0.0.1"))
        ("p,port", "Port to be used", cxxopts::value<unsigned short>()->default_value("51366"))
    ;
    options.parse_positional({"user", "server", "port"});

    // Input parsing block
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
        
    } catch (const cxxopts::OptionSpecException& e) {
        std::cerr << "Error while parsing options:\n"
        << e.what() << std::endl;
        exit(1);

    } catch (const cxxopts::OptionParseException& e) {
        std::cerr << "Error while parsing options:\n"
        << e.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Error while parsing options:\n"
        << "Unexpected exception occurred at parser." << std::endl;
        exit(1);
    }

    // If the help flag was set, prints the help message and exits
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    // Start of the sanity check block
    bool error = false;
    std::stringstream errorMessage;

    // Username checking block
    if (result.count("user") == 0) {
        errorMessage << "Error: Username must be provided" << std::endl;
        error = true;
    }
    if (result.count("user")) {
        if (result.count("user") == 1) {
            auto username = result["user"].as<std::string>();

            // Checking for invalid length
            size_t length = username.length();
            if (length < 4) {
                errorMessage << "Error: Provided username '"
                << username
                << "' is too short. Usernames must comprise 4 to 20 characters."
                << std::endl;
                
                error = true;

            } else if (length > 20) {
                errorMessage << "Error: Provided username '"
                << username
                << "' is too long. Usernames must comprise 4 to 20 characters." << std::endl;
            
                error = true;
            }

            // Checking for invalid chars
            bool nonAlpha = !std::regex_match(username, std::regex("^[A-Za-z]+$"));
            if (nonAlpha) {
                errorMessage << "Error: Provided username '"
                << username
                << "' contains non-letter characters." << std::endl;

                error = true;
            }

        } else { // Username was provided more than once
            errorMessage << "Error: more than one username was provided." << std::endl;
            error = true;
        }
    }

    // Server address can be set only once
    if (result.count("server") > 1) {
        errorMessage << "Error: Server address was set more than once." << std::endl;
        error = true;
    }

    if (result.count("server")) {
        auto server = result["server"].as<std::string>();

        bool invalid = !std::regex_match(server, std::regex("^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}$"));
        
        if (invalid) {
            errorMessage << "Error: Provided server address '"
            << server
            << "' is invalid." << std::endl;

            error = true;
        }
    }

    // Port can be set only once
    if (result.count("port") > 1) {
        errorMessage << "Error: Port setting is provided more than once." << std::endl;
        error = true;
    }

    // If an error was encountered, prints the errors and exits
    if (error) {
        std::cerr << errorMessage.str() 
        << "Finishing execution."
        << std::endl;
        exit(0);
    }
    
    // Input sanity check finished
    return result;
}

cxxopts::ParseResult parseServer(int argc, char* argv[]) {
    cxxopts::Options options("IncrebileTvitter", "Welcome to IncredibleTviter Server!");

    // Add options here
    options.add_options()
        ("h,help", "Print usage")
        ("p,port", "Port to be used", cxxopts::value<unsigned short>()->default_value("51366"))
    ;

    // Input parsing block
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
        
    } catch (const cxxopts::OptionSpecException& e) {
        std::cerr << "Error while parsing options:\n"
        << e.what() << std::endl;
        exit(1);

    } catch (const cxxopts::OptionParseException& e) {
        std::cerr << "Error while parsing options:\n"
        << e.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Error while parsing options:\n"
        << "Unexpected exception occurred at parser." << std::endl;
        exit(1);
    }

    // If the help flag was set, prints the help message and exits
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    // Start of the sanity check block
    bool error = false;
    std::stringstream errorMessage;

    // Port can be set only once
    if (result.count("port") > 1) {
        errorMessage << "Error: Port setting is provided more than once." << std::endl;
        error = true;
    }

    // If an error was encountered, prints the errors and exits
    if (error) {
        std::cerr << errorMessage.str() 
        << "Finishing execution."
        << std::endl;
        exit(0);
    }
    
    // Input sanity check finished
    return result;
}
