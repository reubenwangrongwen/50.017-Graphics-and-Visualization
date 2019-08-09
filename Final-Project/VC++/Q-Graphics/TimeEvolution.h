#ifndef TIMEEVOLUTION_H
#define TIMEEVOLUTION_H

#include <vector>
#include <vecmath.h>

#include "ClothSystem.h"


class TimeEvolution : public ClothSystem {

public:
	void tEvolve(double dt);
};

#endif
#pragma once
