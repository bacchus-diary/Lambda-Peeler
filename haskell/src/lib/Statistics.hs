module Statistics where

import Data.List
import Data.Tuple
import Debug.Trace
import Text.Printf
import Foreign.C.Types
import Foreign.Ptr
import Foreign.Marshal

foreign export ccall hsAroundCenter :: CDouble -> CInt -> Ptr CDouble -> IO (Ptr CDouble)
hsAroundCenter proportion len srcArray = do
    src <- peekArray (fromIntegral len) srcArray
    let (a, b) = aroundCenter (realToFrac proportion) (map realToFrac src)
    newArray $ map realToFrac [a, b]

aroundCenter :: Double -> [Double] -> (Double, Double)
aroundCenter _ [] = (0, 0)
aroundCenter proportion src
    | proportion >= 1 = (head sorted, last sorted)
    | proportion > 0 = check target 0.1 $ maximum [diff $ head sorted, diff $ last sorted]
    | otherwise = error $ "Illegal proportion: " ++ show(proportion)
    where
        num = realToFrac(length src)
        target = round(proportion * num)
        sorted = sort src
        bottom = head sorted - 1
        cv = sorted !! round(num / 2) - bottom
        diff v = abs((v - bottom) / cv - 1)
        value rate = cv * (1 + rate) + bottom
        check preGap preRate rate
            | (preRate == rate || gap == 0) = (minValue, maxValue)
            | rate >= 0 = check gap rate postRate
            | otherwise = error $ "Illegal rate: " ++ show(rate)
            where
                (minValue, maxValue) = (value(-rate), value(rate))
                count = length $ filter (\v -> minValue <= v && v <= maxValue) sorted
                gap = target - count
                postRate =
                    let r = abs(preRate / rate - 1) / if gap * preGap < 0 then 10 else 1
                    in rate + realToFrac(signum gap) * r * rate
