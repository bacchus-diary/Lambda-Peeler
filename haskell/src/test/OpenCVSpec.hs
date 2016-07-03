module OpenCVSpec where

import Test.Hspec

import CompareKeys

spec :: Spec
spec = do
    describe "Compare" $ do
        it "keypoints" $ do
            compareKeypoints 1
            print "done."
