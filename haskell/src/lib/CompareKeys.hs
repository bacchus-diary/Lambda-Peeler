module CompareKeys where

import Text.Printf
import Foreign.C.Types

foreign export ccall hsCompareKeypoints :: CInt -> IO CInt
hsCompareKeypoints = fmap fromIntegral . compareKeypoints . fromIntegral

compareKeypoints :: Int -> IO Int
compareKeypoints count = do
    printf "Entering 'compareKeypoints' [%d]...\n" count
    return count
