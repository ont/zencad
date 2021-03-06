import zencad
import pyservoce

import zencad.util

def arrow(pnt, vec, clr=zencad.color.white, arrlen = 5, width = 1):
	return pyservoce.draw_arrow(zencad.util.point3(pnt), zencad.util.vector3(vec), clr, arrlen, width)


def line(apnt, bpnt, clr=zencad.color.white, width = 1):
	return pyservoce.draw_line(zencad.util.point3(apnt), zencad.util.point3(bpnt), clr=clr, width=width)