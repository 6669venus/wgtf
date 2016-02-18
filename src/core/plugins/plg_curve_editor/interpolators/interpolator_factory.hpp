//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  interpolator_factory.hpp
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Copyright (c) Wargaming.net. All rights reserved.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef INTERPOLATOR_FACTORY_
#define INTERPOLATOR_FACTORY_

#pragma once

#include "interfaces/curve_types.hpp"
#include "interfaces/i_curve_interpolator.hpp"

ICurveInterpolatorPtr createInterpolator(CurveTypes::CurveType);

#endif INTERPOLATOR_FACTORY_