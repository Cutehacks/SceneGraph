/****************************************************************************
**
** Copyright (C) 2014 Cutehacks AS.
** Contact: http://www.cutehacks.com/contact
**
****************************************************************************/

#include "rasterizer.h"

#if !RAW_RASTERIZER
QOpenGLFunctions *Rasterizer::m_current = 0;
#endif
