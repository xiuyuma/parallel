
#' iterative pruning empirical bayesian
#'
#' @param data single cell expression matrix, row as genes column as cells (normalized counts required)
#' @param conditions partition of cells
#' @param uc number of unceratin relations between means of subtypes for each gene level
#' @param r r of NB
#' @param iLabel label for isoform, indicating which beta are shared
#' @param iter maximum iteration step of EM
#' @param alpha start point of hyper parameter alpha
#' @param beta start point of hyper parameter beta
#' @param step1 stepsize for gradient ascent of alpha
#' @param step2 stepsize for gradietn ascent of beta
#' @param thre threshold for determining whether a relation is sure or uncertain
#' @param sthre shrinkage threshold for iterative pruning space of DE patterns
#' @param filter filterthreshold for low expression gene for DE analysis
#' @param stopthre stopping threshold for EM
#' @return a list containing considered DE patterns and their posterior probability, values for alpha and beta
#' @export

EBSeqTestWithQ <- function(data,conditions,uc,r,iLabel = 1,sizefactor = 1,
iter = 50,alpha = 0.4, beta = 0, step1 = 1e-6,step2 = 0.01,
thre = log(2), sthre = 0.001, filter = 10, stopthre = 1e-3) {
    
    if(!is.matrix(data))
    {
        stop("data must be a numerical matrix")
    }
    if(length(conditions) != ncol(data))
    {
        stop("incorrect length of conditions")
    }
    if(beta != 0)
    {
        if(length(beta) != nrow(data)){stop("incorrect length of hyper parameters")}
    }
    if(beta == 0)
    {
        beta = rep(2,nrow(data))
    }
    if(length(r) != nrow(data))
    {
        stop("incorrect length of r")
    }
    if(length(iLabel) == 1 && iLabel == 1)
    {
        iLabel = 1:nrow(data)
    }
    if(length(iLabel) != nrow(data))
    {
        stop("incorrect length of isoform label")
    }
    if(length(sizefactor) == 1 && sizefactor == 1)
    {
        sizefactor = rep(1,ncol(data))
    }
    if(length(sizefactor) != ncol(data))
    {
        stop("incorrect length of size factor")
    }
    
    
    
    .Call('EBSeqWQ',
    scExpMatrix = data,
    groupLabel = conditions,
    r = r,
    iLabel = iLabel,
    sizeFactor = sizefactor,
    iter = iter,
    alpha = alpha,
    beta = beta,
    step1 = step1,
    step2 = step2,
    uc = uc,
    thre = thre,
    sthre = sthre,
    filter = filter,
    stopthre = stopthre)
    
    
}



