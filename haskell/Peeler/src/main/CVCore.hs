{-# LANGUAGE TemplateHaskell #-}
{-# LANGUAGE QuasiQuotes #-}

module CVCore where

import Language.C.Inline.Cpp as C

C.context C.cppCtx

C.include "<opencv/cv.hpp>"

shout :: Float -> IO Float
shout x = do
  let v = realToFrac x
  r <- [C.exp| float {
      cv::cubeRoot($(float v))
    } |]
  return $ realToFrac r
