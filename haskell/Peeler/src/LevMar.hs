module LevMar(levmarDif) where

import Foreign.C.Types
import Foreign.Ptr
import Foreign.Marshal.Array
import Foreign.Marshal.Alloc

data CVoid

type MeasureFunc = Ptr CDouble -> Ptr CDouble -> CInt -> CInt -> Ptr CVoid -> IO ()

foreign import ccall "wrapper" wrapper :: MeasureFunc -> IO (FunPtr MeasureFunc)

foreign import ccall "levmar dlevmar_dif" dlevmar_dif ::
  FunPtr MeasureFunc ->
  Ptr CDouble -> Ptr CDouble -> CInt -> CInt -> CInt ->
  Ptr CDouble -> Ptr CDouble -> Ptr CDouble -> Ptr CDouble -> Ptr CVoid -> IO ()

levmarDif :: Int -> ([Double] -> [Double]) -> Int -> [Double] -> IO [Double]
levmarDif c meas max src = do
  s <- newArray $ map realToFrac src
  m <- wrapper measure
  dlevmar_dif m s nullPtr (fromIntegral $ length src) (fromIntegral c) (fromIntegral max) nullPtr nullPtr nullPtr nullPtr nullPtr
  freeHaskellFunPtr m
  r <- peekArray (length src) s
  free s
  return $ map realToFrac r
  where
    measure p hx m n adata = do
      a <- peekArray (length src) p
      pokeArray hx $ map realToFrac $ meas $ map realToFrac a
