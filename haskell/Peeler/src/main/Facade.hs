module Facade where

import Foreign.C.Types
import Foreign.C.String

foreign import ccall "loadVideo" loadVideo :: CString -> IO ()

foreign export ccall start :: CString -> IO CString
start :: CString -> IO CString
start moviePath = do
    compareKeypoints 0
    return moviePath

foreign export ccall compareKeypoints :: CInt -> IO ()
compareKeypoints :: CInt -> IO ()
compareKeypoints count = do
    print "Entering 'compareKeypoints'..."
    print count
