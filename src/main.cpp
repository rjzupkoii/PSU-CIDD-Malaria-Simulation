/* 
 * File:   main.cpp
 * Author: Merlin
 *
 * Created on August 1, 2013, 8:51 AM
 */

#include "Model.h"
#include "Reporters/ConsoleReporter.h"
#include "ezOptionParser.hpp"


using namespace std;

void CreateOptionParser(ez::ezOptionParser &opt);

void Usage(ez::ezOptionParser &opt) {
    std::string usage;
    opt.getUsage(usage);
    std::cout << usage;
    std::cout << "=========================" << std::endl;
};

void handle_option_parser(Model *m, ez::ezOptionParser &opt);

/*
 * 
 */
int main(int argc, const char *argv[]) {
    try {
        Model *m = new Model();

        ez::ezOptionParser opt;
        CreateOptionParser(opt);
        opt.parse(argc, argv);

        if (opt.isSet("-h")) {
            std::string usage;
            int layout;
            opt.get("-h")->getInt(layout);
            switch (layout) {
                case 0:
                    opt.getUsage(usage, 80, ez::ezOptionParser::ALIGN);
                    break;
                case 1:
                    opt.getUsage(usage, 80, ez::ezOptionParser::INTERLEAVE);
                    break;
                case 2:
                    opt.getUsage(usage, 80, ez::ezOptionParser::STAGGER);
                    break;
            }
            std::cout << usage;
            return 1;
        }

        std::vector<std::string> badOptions;
        int i;
        if (!opt.gotRequired(badOptions)) {
            for (i = 0; i < badOptions.size(); ++i)
                std::cerr << "ERROR: Missing required option " << badOptions[i] << ".\n\n";
            Usage(opt);
            return 0;
        }

        //        if (!opt.gotExpected(badOptions)) {
        //            for (i = 0; i < badOptions.size(); ++i)
        //                std::cerr << "ERROR: Got unexpected number of arguments for option " << badOptions[i] << ".\n\n";
        //
        //            Usage(opt);
        //            return 1;
        //        }


        handle_option_parser(m, opt);

        //        m->set_initial_seed_number(1377584145);

        m->initialize();

        //    

        m->run();


        delete m;
    } catch (const std::exception &e) {
        std::cout << __FUNCTION__ << "-" << e.what() << std::endl;
    }
    return 0;
}

void CreateOptionParser(ez::ezOptionParser &opt) {
    opt.overview = "Individual-based simulation for malaria.";
    opt.syntax = "malariasimulation [OPTIONS]";
    opt.example = "malariasimulation -h\n\n";
    opt.footer = "=========================\nmalariasimulation v3.0.2\n";

    opt.add(
            "0", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Display usage instructions.\nThere is a choice of three different layouts for description alignment. Your choice can be any one of the following to suit your style:\n\n0 - align (default)\n1 - interleave\n2 - stagger", // Help description.
            "-h", // Flag token. 
            "-help", // Flag token.
            "--help", // Flag token.
            "--usage" // Flag token.
    );
    opt.add(
            "", // Default.
            0, // Required?
            0, // Number of args expected.
            ' ', // Delimiter if expecting multiple args.
            "Run the simulation with farm output.", // Help description.
            "-f", // Flag token. 
            "-farm", // Flag token.
            "--farm", // Flag token.
            "-ffo"
    );

    opt.add(
            "config.yml", // Default.
            1, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "The config file (YAML format). \nEx: malariasimulation -i config.yml", // Help description.
            "-i", // stand for input
            "-c", // config
            "--config",
            "--input" // Flag token.
    );

    opt.add(
            "tme.txt", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "The tme scheduler file (CSV format).", // Help description.
            "-tme" // Flag token. 
    );
    opt.add("override.txt",
            0,
            1,
            0,
            "Provide file name and line number to overwrite input parameters",
            "-o",
            "--override"
    );

    opt.add("1",
            0,
            1,
            0,
            "Provide line number to overwrite input parameters",
            "-l",
            "-p",
            "--line"
    );

    opt.add("-1",
            0,
            1,
            0,
            "Seed number for random generator",
            "-s",
            "--seed"
    );

    opt.add("0",
            0,
            1,
            0,
            "Output for GUI",
            "-g",
            "-gui",
            "--gui"
    );
}

void handle_option_parser(Model *m, ez::ezOptionParser &opt) {
    std::string config_filename = "config.yml";
    if (opt.isSet("-i")) {

        opt.get("-i")->getString(config_filename);
        //        std::cout << config_filename << std::endl;
    }
    m->set_config_filename(config_filename);

    std::string tme_filename = "C:\\Users\\Merlin\\Documents\\My Box Files\\Works\\MaSim_TME\\MalariaSimulation\\tme.txt";
    if (opt.isSet("-tme")) {
        opt.get("-tme")->getString(tme_filename);
    }
    m->set_tme_filename(tme_filename);

    std::string override_filename = "";
    int line_number = -1;
    if (opt.isSet("-o")) {
        std::string override_param;
        opt.get("-o")->getString(override_param);
        override_filename = override_param;
        //        std::cout << override_filename << std::endl;

        opt.get("-l")->getString(override_param);
        line_number = atoi(override_param.c_str());
        //        std::cout << line_number << std::endl;
    }

    m->set_override_parameter_filename(override_filename);
    m->set_override_parameter_line_number(line_number);

    long seed = -1;
    if (opt.isSet("-s")) {
        opt.get("-s")->getLong(seed);
        //        std::cout << seed << std::endl;
    }
    m->set_initial_seed_number(seed);

    int gui_type = 0;
    if (opt.isSet("-gui")) {
        opt.get("-gui")->getInt(gui_type);
        m->set_gui_type(gui_type);
        std::cout << gui_type << std::endl;
    }

    if (opt.isSet("-f")) {
        m->set_is_farm_output(true);
    }

}