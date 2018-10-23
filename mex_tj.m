c = mex.compilers.gcc('./tj.c');
c.addLib('openmp');
c.addCompileFlags('-mavx2');
c.addCompileFlags('-mabm');
c.addCompileFlags('-mbmi');
c.addCompileFlags('-mbmi2');

%c.addCompileFlags('-march=native');

c.build();