fd = open("Grace_kid.py", "w")
c = "fd = open(%cGrace_kid.py%c, %cw%c)%cc = %c%s%c%c%c%c%c%c%ccommentaire%c%c%c%c%cfd.write(c %c (34, 34, 34, 34, 10, 34, c, 34, 10, 34, 34, 34, 10, 9, 10, 34, 34, 34, 10, 37))"
"""
	commentaire
"""
fd.write(c % (34, 34, 34, 34, 10, 34, c, 34, 10, 34, 34, 34, 10, 9, 10, 34, 34, 34, 10, 37))