#include "move.hpp"
#include "position.hpp"
#include "benchmark.hpp"
#include "mcts.hpp"

int main(int argc, char* argv[]) {
	felix::Position pos;
	felix::MCTS::Node* root = new felix::MCTS::Node(&pos);
	for(int i = 0; i < 5000000; i++) {
		root->simulate();
		if((i + 1) % 10000 == 0) {
			printf("Complete %d simulations\n", i + 1);
		}
	}
	root->info(std::cerr);
	std::cout << "bestmove = " << root->getBestMove() << std::endl;
	return 0;
}
