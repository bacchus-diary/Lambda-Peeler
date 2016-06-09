module LevMarSpec where

import Test.Hspec
import LevMar

spec :: Spec
spec = do
  describe "Levenberg Marquardt" $ do
    it "dif" $ do
      result <- levmarDif 3 calc 300 [0, 0]
      result `shouldBe` [0.799952932, 0.199988709]
      where
        points = [(-3.1, 0.18), (0.3, 0.86), (1.2, 1.04)]
        calc params = map (diff params) points
        diff [p0, p1] (x, y) = p1 * x + p0 - y
