
#include <parser.hpp>

cxxopts::ParseResult parse(int argc, char* argv[]) {
    cxxopts::Options options("IncrebileTvitter", "Welcome to IncredibleTviter!");

    // Add options here
    options.add_options()
        ("h,help", "Print usage")
        ("s,server", "If set, the program will execute as a server", cxxopts::value<bool>()->default_value("false"))
        ("u,user", "Username with which to sign in", cxxopts::value<std::string>())
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

    // Server or Username must be provided for a valid execution
    if (result.count("server") == 0 && result.count("user") == 0) {
        errorMessage << "Error: Execution requires that either an username be provided "
        << "or that the server flag be set."
        << std::endl;

        error = true;
    }

    // Or Server or Username execution type
    if (result.count("server") > 0 && result.count("user") > 0) {
        errorMessage << "Error: Username was provided for a server, "
        << "but servers and clients are mutually exclusive. "
        << "Please, execute either as a user or as a server."
        << std::endl;

        error = true;
    }

    // Server flag can be set only once
    if (result.count("server") > 1) {
        errorMessage << "Error: Server flag was set more than once." << std::endl;
        error = true;
    }

    // Port can be set only once
    if (result.count("port") > 1) {
        errorMessage << "Error: Port setting is provided more than once." << std::endl;
        error = true;
    }

    // Username checking block
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
