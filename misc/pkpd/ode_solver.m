syms A(t) B(t) C(t) P(t) a x y z k D Ap Bp f(Cd) Vmax n EC50 P0

f(Cd) = Vmax*Cd^n / (Cd^n+EC50^n);
%%
ode1 = diff(A) == -A*x ;
ode2 = diff(B) == A*x - B*y - B*z;
ode3 = diff(C) == B*z-C*k;
ode4 = diff(P) == P(a-f(C));


odes = [ode1; ode2; ode3; ode4];

cond1 = A(0) == D + Ap;
cond2 = B(0) == Bp;
cond3 = C(0) == 0;
cond4 = P(0) == P0;

conds = [cond1; cond2; cond3; cond4];

S = dsolve(odes, conds);
%%

ASol(t) = S.A
BSol(t) = S.B
CSol(t) = S.C
PSol(t) = S.P
%%