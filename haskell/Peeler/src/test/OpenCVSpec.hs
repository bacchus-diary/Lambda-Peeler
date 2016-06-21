module OpenCVSpec where

import Test.Hspec

import Foreign.C.String

import Facade

spec :: Spec
spec = do
    describe "OpenCV" $ do
        it "start" $ do
            withCString "resources/test/opencv/pickled_cucumbers.mp4" $ \c_str ->
                start c_str
            print "done."
