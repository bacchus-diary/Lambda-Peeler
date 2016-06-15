module OpenCVSpec where

import Test.Hspec
import CVCore

spec :: Spec
spec = do
  describe "OpenCV" $ do
    it "shout" $ do
      result <- shout 27
      result `shouldBe` 3
