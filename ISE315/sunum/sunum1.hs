import System.Environment
import qualified Data.MemoCombinators as Memo

lcs [] _ = 0
lcs _ [] = 0
lcs (f:fs) (s:ss) 
	| f==s = 1 + lcs fs ss
	| f/=s = max (lcs fs (s:ss)) (lcs (f:fs) ss) 
	
dyn_lcs = (Memo.memo2 (Memo.list Memo.char) (Memo.list Memo.char)) dyn_lcs'
	where
		dyn_lcs' [] _ = 0
		dyn_lcs' _ [] = 0
		dyn_lcs' (f:fs) (s:ss) 
			| f==s = 1 + dyn_lcs fs ss
			| f/=s = max (dyn_lcs fs (s:ss)) (dyn_lcs (f:fs) ss) 
	
main = do
	arg1:arg2:rest <- getArgs
	if length rest == 0 then do
		_ <- print "dynamic programming haskell:"
		print (dyn_lcs arg1 arg2)
	else do
		_ <- print "recursive programming haskell:"
		print (lcs arg1 arg2)
	