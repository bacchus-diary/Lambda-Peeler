module OpenCVSpec where

import Test.Hspec
import OpenCV.Basic

spec :: Spec
spec = do
  describe "OpenCV" $ do
    it "feature" $ do
      img <- readImage "Some"
      detect "Some"
      print img
