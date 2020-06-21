data Exp v o =   Var v              -- expressions are either variables
               | Term o [ Exp v o ] -- or terms involving operators and
                                    -- subterms
               deriving (Show,Eq,Ord)

dic_rd :: Dict -> String -> String
dic_rd (Var v) [] = v
dic_rd l [] = ""
dic_rd (Term o ((Term a l):t)) (h:tt) = if (h == a) then dic_rd l tt else dic_rd (Term o t) (h:tt)
