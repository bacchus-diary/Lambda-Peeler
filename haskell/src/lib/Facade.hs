module Facade where

import Foreign.C.String

foreign import ccall loadVideo :: CString -> IO ()

foreign export ccall start :: CString -> IO CString
start filepath = do
    loadVideo filepath
    return filepath

hsLoadVideo :: String -> IO ()
hsLoadVideo n = withCString n loadVideo
