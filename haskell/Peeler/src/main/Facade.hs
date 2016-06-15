module Facade where

import Foreign.C.Types
import Foreign.C.String

foreign export ccall start :: CString -> CString

start :: CString -> CString
start moviePath = moviePath
