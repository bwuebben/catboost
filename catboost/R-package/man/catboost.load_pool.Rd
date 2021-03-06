% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/catboost.R
\name{catboost.load_pool}
\alias{catboost.load_pool}
\title{Create a dataset}
\usage{
catboost.load_pool(data, label = NULL, cat_features = NULL,
  column_description = NULL, pairs = NULL, delimiter = "\\t",
  has_header = FALSE, weight = NULL, group_id = NULL,
  subgroup_id = NULL, pairs_weight = NULL, baseline = NULL,
  feature_names = NULL, thread_count = -1)
}
\arguments{
\item{data}{A file path, matrix or data.frame with features.
The following column types are supported:
\itemize{
    \item double
    \item factor.
    It is assumed that categorical features are given in this type of columns.
    A standard CatBoost processing procedure is applied to this type of columns:
    \describe{
        \item{1.}{The values are converted to strings.}
        \item{2.}{The ConvertCatFeatureToFloat function is applied to the resulting string.}
    }
}

Default value: Required argument}

\item{label}{The label vector.}

\item{cat_features}{A vector of categorical features indices.
The indices are zero based and can differ from the given in the Column descriptions file.}

\item{column_description}{The path to the input file that contains the column descriptions.}

\item{pairs}{A file path, matrix or data.frame that contains the pairs descriptions. The shape should be Nx2, where N is the pairs' count.
The first element of pair is the index of winner document in training set. The second element of pair is the index of loser document in training set.}

\item{delimiter}{Delimiter character to use to separate features in a file.}

\item{has_header}{Read column names from first line, if this parameter is set to True.}

\item{weight}{The weights of the objects.}

\item{group_id}{The group ids of the objects.}

\item{subgroup_id}{The subgroup ids of the objects.}

\item{pairs_weight}{The weights of the pairs.}

\item{baseline}{Vector of initial (raw) values of the target function for the object.
Used in the calculation of final values of trees.}

\item{feature_names}{A list of names for each feature in the dataset.}

\item{thread_count}{The number of threads to use while reading the data. Optimizes reading time. This parameter doesn't affect results.
If -1, then the number of threads is set to the number of cores.}
}
\value{
catboost.Pool
}
\description{
Create a dataset from the given file, matrix or data.frame.
}
\examples{
# From file
pool_path <- system.file("extdata", "adult_train.1000", package = "catboost")
test_pool_path <- system.file("extdata", "adult_test.1000", package = "catboost")
cd_path <- system.file("extdata", "adult.cd", package = "catboost")
pool <- catboost.load_pool(pool_path, column_description = cd_path)
test_pool <- catboost.load_pool(test_pool_path, column_description = cd_path)

# From matrix
pool_path <- 'train_full3'
data <- read.table(pool_path, head = F, sep = "\\t", colClasses = rep('numeric', 10))
target <- c(1)
cat_features <- seq(1,8)
data_matrix <- as.matrix(data)
pool <- catboost.load_pool(as.matrix(data[,-target]), label = as.matrix(data[,target]), cat_features = cat_features)

# From data.frame
pool_path <- 'train_full3'
cd_vector <- c('numeric',  rep('numeric',2), rep('factor',7))
data <- read.table(pool_path, head = F, sep = "\\t", colClasses = cd_vector)
target <- c(1)
learn_size <- floor(0.8 * nrow(data))
learn_ind <- sample(nrow(data), learn_size)
learn <- data[learn_ind,]
test <- data[-learn_ind,]
learn_pool <- catboost.load_pool(learn[,-target], label = learn[,target])
test_pool <- catboost.load_pool(test[,-target], label = test[,target])

}
