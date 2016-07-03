module LevMarSpec where

import Test.Hspec
import LevMar

spec :: Spec
spec = do
    describe "Levenberg Marquardt" $ do
        it "dif" $ do
            result <- levmarDif 3 calc 1000 [0, 0]
            result `shouldBe` [0.7999999999996789, 0.1999999999999391]
            where
                points = [(-3.1, 0.18), (0.3, 0.86), (1.2, 1.04)]
                calc params = map (diff params) points
                diff [p0, p1] (x, y) = p1 * x + p0 - y
