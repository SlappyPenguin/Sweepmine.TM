MODE ?= normal

ifeq ($(MODE), normal)
	TARGET = sweepmine_normal
else ifeq ($(MODE), modded)
	TARGET = sweepmine_modded
else
	$(error Invalid MODE specified. Use 'normal' or 'fast'.)
endif

all: $(TARGET)

sweepmine_normal:
	g++ -O3 ../src/final/normal_all.cpp ../src/final/grader.cpp \
	-march=native -flto -funroll-loops \
	-std=c++20 \
	-o ../bin/sweepmine

sweepmine_modded:
	g++ -O3 ../src/final/modded_all.cpp ../src/final/grader.cpp \
	-march=native -flto -funroll-loops \
	-std=c++20 \
	-o ../bin/sweepmine