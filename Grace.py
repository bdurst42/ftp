import sys
from parser import cv1_parse_equation
from solve import cv1_solve
def main():
	if len(sys.argv) > 2:
		print "Too many arguments !"
	elif len(sys.argv) < 2:
		print "Too few arguments !"
	else:
		cv1_solve(cv1_parse_equation(sys.argv[1]))

main()
