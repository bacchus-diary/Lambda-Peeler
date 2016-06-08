#!/bin/bash

AWS_LAMBDA_FUNC_NAME=${1%%:*}
AWS_LAMBDA_ALIAS_NAME=${1##*:}
AWS_S3_BUCKET=${2%%/*}
AWS_S3_FILEPATH=${2#*/}
ZIP_NAME=${2##*/}

json() {
  python -c "import sys,json; print(json.loads(sys.stdin.read())['$1'])"
}

PRE=$(aws lambda get-alias --function-name $AWS_LAMBDA_FUNC_NAME --name $AWS_LAMBDA_ALIAS_NAME | json FunctionVersion)

aws s3 cp $ZIP_NAME s3://$AWS_S3_BUCKET/$AWS_S3_FILEPATH > /dev/null || exit 1

VERSION=$(aws lambda update-function-code --function-name $AWS_LAMBDA_FUNC_NAME --s3-bucket $AWS_S3_BUCKET --s3-key $AWS_S3_FILEPATH --publish | json Version)
[ -z $VERSION ] && exit 1

CU='update'
if [ -z $PRE ]
then
  PRE=$VERSION
  CU='create'
fi

rename() {
  aws lambda $CU-alias --function-name $AWS_LAMBDA_FUNC_NAME --name $1 --function-version $2 > /dev/null 2>&1 || exit 1
}
rename $AWS_LAMBDA_ALIAS_NAME $VERSION
rename "${AWS_LAMBDA_ALIAS_NAME}_pre" $PRE

echo $VERSION
