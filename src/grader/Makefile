all: normal_graded weighted_torus_graded

normal_graded:
	g++ grader.cpp ../normal/graded.cpp \
	../normal/logic.cpp ../normal/constraints.cpp ../normal/gauss.cpp ../normal/probability.cpp \
	-Wall -O3 -march=native -flto -funroll-loops \
	-o ../../build/normal_graded

weighted_torus_graded:
	g++ grader.cpp ../weighted_torus/graded.cpp \
	../weighted_torus/constraints.cpp ../weighted_torus/probability.cpp \
	-Wall -O3 -march=native -flto -funroll-loops \
	-o ../../build/weighted_torus_graded