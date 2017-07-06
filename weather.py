# -*- coding: utf-8 -*-

import subprocess

cmd = "git log --since='6am' --oneline | wc -l"
ps = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
output = ps.communicate()[0]
print(str(int(output)))
