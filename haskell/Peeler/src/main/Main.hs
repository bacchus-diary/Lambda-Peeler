import System.Environment (getArgs)
import Foreign.C.String

import Facade

main = do
    args <- getArgs
    withCString (concat args) start
