module Math where

import Foreign.C.Types

foreign export ccall calcSin :: CDouble -> CDouble
foreign import ccall "sin" c_sin :: CDouble -> CDouble

cSin :: Double -> Double
cSin = realToFrac . c_sin . realToFrac

calcSin :: CDouble -> CDouble
calcSin = realToFrac . cSin . realToFrac
