module CompareKeys where

import Foreign.C.Types

foreign export ccall hsCompareKeypoints :: CInt -> IO CInt

hsCompareKeypoints :: CInt -> IO CInt
hsCompareKeypoints n = do
    r <- compareKeypoints $ fromIntegral n
    return $ fromIntegral r

compareKeypoints :: Int -> IO Int
compareKeypoints count = do
    print "Entering 'compareKeypoints'..."
    print count
    return count
