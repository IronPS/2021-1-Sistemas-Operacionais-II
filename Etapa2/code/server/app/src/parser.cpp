
#include <parser.hpp>

cxxopts::ParseResult parse(int argc, char* argv[]) {
    cxxopts::Options options("IncrebileTvitter", "Welcome to IncredibleTviter Server!");

    // Add options here
    options.add_options()
        ("h,help", "Print usage")
        ("p,port", "Port that will treat clients", cxxopts::value<unsigned short>()->default_value("51366"))
        ("i,id", "The ID of this replica", cxxopts::value<unsigned short>()->default_value("0"))
        ("d,ids", "List with the IDs of all replicas separated by commas with no spaces in-between", cxxopts::value<std::vector<unsigned short>>()->default_value(""))
        ("a,addresses", "List with the addresses of all replicas separated by commas with no spaces in-between", cxxopts::value<std::vector<std::string>>()->default_value(""))
        ("r,auxports", "List with the auxiliary ports of all replicas separated by commas with no spaces in-between", cxxopts::value<std::vector<unsigned short>>()->default_value(""))
        ("s,cliports", "List with the client ports of all replicas separated by commas with no spaces in-between", cxxopts::value<std::vector<unsigned short>>()->default_value(""))
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
