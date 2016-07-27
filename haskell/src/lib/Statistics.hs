module Statistics where

import Data.List
import Data.Tuple
import Debug.Trace
import Text.Printf
import Foreign.C.Types
import Foreign.Ptr
import Foreign.Marshal

foreign export ccall hsAroundCenter :: CDouble -> CInt -> Ptr CDouble -> IO (Ptr CDouble)
hsAroundCenter rate len srcArray = do
    src <- fmap (map realToFrac) $ peekArray (fromIntegral len) srcArray
    printf "Sorting src array: %d\n" (length src)
    let dst = aroundCenter (realToFrac rate) src
    newArray $ map realToFrac [fst dst, snd dst]

aroundCenter :: Double -> [Double] -> (Double, Double)
aroundCenter rate src =
    check target 0.1 $ maximum [diff $ head sorted, diff $ last sorted]
    where
        num = realToFrac(length src)
        target = round(rate * num)
        sorted = sort src
        cv = sorted !! round(num / 2)
        diff v = abs(v / cv - 1)
        interval d = (cv * (1 - d), cv * (1 + d))
        count (a, b) = length $ filter (\v -> a <= v && v <= b) sorted
        check preGap preRate rate =
            if gap == 0 || preRate == rate
                then t
                else check gap rate $ rate + realToFrac(signum gap) * r * rate
            where
                t = interval rate
                gap = target - (count t)
                r = abs(preRate / rate - 1) / if gap * preGap < 0 then 10 else 1
