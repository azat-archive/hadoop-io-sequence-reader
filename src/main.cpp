
/**
 * This file is part of the hadoop-io-sequence-reader package.
 *
 * (c) Azat Khuzhin <a3at.mail@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include <string>
#include <ostream>
#include <fstream>
#include <istream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#include "reader.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " FILE" << endl;
        return EXIT_FAILURE;
    }

    ifstream input(argv[1], std::ifstream::binary);
    assert(input);

    try {
        Reader reader(&input);
        while (reader.next())
        {
            std::cout << reader;
        }
    } catch (const string &exceptionString) {
        cerr << exceptionString << endl;
    }

    return EXIT_SUCCESS;
}
