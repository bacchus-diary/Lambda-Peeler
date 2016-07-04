module OpenCVSpec where

import Test.Hspec

import Facade
import CompareKeys

spec :: Spec
spec = do
    describe "Compare" $ do
        it "keypoints" $ do
            compareKeypoints 1
            print "done."
        it "loadVideo" $ do
            hsLoadVideo "resources/test/pickled_cucumbers.mp4"
