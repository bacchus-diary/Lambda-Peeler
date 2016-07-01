import System.Environment (getArgs)
import Foreign.C.String

foreign import ccall "loadVideo" loadVideo :: CString -> IO ()

main = do
    args <- getArgs
    withCString (concat args) loadVideo
