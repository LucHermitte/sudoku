#include <iostream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <stdexcept>
#include "sudoku/grid.hpp"
#include "sudoku/solver.hpp"

int main(int argc, char **argv)
{
    try
    {
        if (argc < 2) {
            throw std::runtime_error("Argument expected.");
        }
        std::string const filename = argv[1];
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Cannot open "+filename);
        }

        grid gr;
        if (! gr.read(file)) {
            throw std::runtime_error("Cannot read a valid grid from "+filename);
        }
        std::cout << "Starting from:\n" << gr << "\n";
        std::cout << (gr.is_solved() ? "already solved" : "to be solved") << "\n";

        int nb_explored = 0;
        auto solution = brute_force(gr, nb_explored);
        if (!solution) {
            std::cout << "This grid has no solution\n";
        }
        else {
            std::cout << "Solution found:\n";
            std::cout << *solution;
        }
        std::cout << "Number of explored states: " << nb_explored << "\n";

        return EXIT_SUCCESS;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return EXIT_FAILURE;
}
