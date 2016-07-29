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
    src <- peekArray (fromIntegral len) srcArray
    let (a, b) = aroundCenter (realToFrac rate) (map realToFrac src)
    newArray $ map realToFrac [a, b]

aroundCenter :: Double -> [Double] -> (Double, Double)
aroundCenter _ [] = (0, 0)
aroundCenter rate src =
    check target 0.1 $ maximum [diff $ head sorted, diff $ last sorted]
    where
        num = realToFrac(length src)
        target = round(rate * num)
        sorted = sort src
        cv = sorted !! round(num / 2)
        diff v = abs(v / cv - 1)
        check preGap preRate rate =
            if gap == 0 || preRate == rate
                then (minValue, maxValue)
                else check gap rate $ rate + realToFrac(signum gap) * r * rate
            where
                (minValue, maxValue) = (cv * (1 - rate), cv * (1 + rate))
                count = length $ filter (\v -> minValue <= v && v <= maxValue) sorted
                gap = target - count
                r = abs(preRate / rate - 1) / if gap * preGap < 0 then 10 else 1
