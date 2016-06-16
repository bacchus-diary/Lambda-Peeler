{-# LANGUAGE TemplateHaskell #-}
{-# LANGUAGE QuasiQuotes #-}

module OpenCV.Basic where

import Language.C.Inline.Cpp as C
import Foreign.C.String
import Foreign.C.Types
import Foreign.Ptr
import Data.Monoid ((<>))
import Data.ByteString.Char8 as BS
import Data.Vector.Storable as V
import Data.Vector.Storable.Mutable as VM

C.context (C.cppCtx <> C.bsCtx <> C.vecCtx)

C.include "opencv2/core.hpp"
C.include "opencv2/highgui.hpp"
C.include "opencv2/features2d.hpp"
C.include "opencv2/xfeatures2d.hpp"

C.using "namespace cv"

sizeOfMat = 96
data Mat = Mat
data KeyPoint = KeyPoint

readImage :: String -> IO (Ptr Mat)
readImage filePath = do
  let file = BS.pack filePath
  p <- [C.block| void * {
    cv::Mat m = cv::imread($bs-ptr:file);
    return &m;
  } |]
  return $ castPtr p

detect :: String -> IO ()
detect filePath = do
  let file = BS.pack filePath
  [C.block| void {
    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(400);
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat m = cv::imread($bs-ptr:file);
    detector->detect(m, keypoints);
  } |]
