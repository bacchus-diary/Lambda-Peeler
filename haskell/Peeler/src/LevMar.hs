module LevMar(levmarDif) where

import Foreign.C.Types
import Foreign.Ptr
import Foreign.Marshal.Array
import Foreign.Marshal.Alloc

data CVoid

type MeasureFunc = Ptr CDouble -> Ptr CDouble -> CInt -> CInt -> Ptr CVoid -> IO ()

foreign import ccall "wrapper" wrapper :: MeasureFunc -> IO (FunPtr MeasureFunc)

foreign import ccall "dlevmar_dif" dlevmar_dif ::
  FunPtr MeasureFunc ->
  Ptr CDouble -> Ptr CDouble -> CInt -> CInt -> CInt ->
  Ptr CDouble -> Ptr CDouble -> Ptr CDouble -> Ptr CDouble -> Ptr CVoid -> IO ()

levmarDif :: Int -> ([Double] -> [Double]) -> Int -> [Double] -> IO [Double]
levmarDif n calc max src = do
  params <- newArray $ map realToFrac src
  func <- wrapper calcDiff
  dlevmar_dif func params nullPtr (fromIntegral $ length src) (fromIntegral n) (fromIntegral max) nullPtr nullPtr nullPtr nullPtr nullPtr
  freeHaskellFunPtr func
  r <- peekArray (length src) params
  free params
  return $ map realToFrac r
  where
    calcDiff params hx m n adata = do
      a <- peekArray (length src) params
      pokeArray hx $ map realToFrac $ calc $ map realToFrac a
