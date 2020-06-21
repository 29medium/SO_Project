import Cp
import Nat

data MTree a = Empty
             | Fork a (MTree a) (MTree a)

a Empty = i1()
a (Fork a e d) = i2(a,(e,d))

rList  f   = id -|- id >< (f >< f)

cataArv g = g . rList (cataArv g) . a
