import optparse
import impala_util as iutil

'''
Train a logistic model using impala.

This creates a table where trained models are stored.
'''


def main():
  # argument parsing
  parser = optparse.OptionParser('usage: %prog LABEL_COL FEATURE_COL [FEATURE_COL ...]')
  parser.add_option("-b", "--db", dest="database", default=None,
                        help="the database which holds data table", metavar="DB")
  parser.add_option("-t", "--table", dest="table", default=None,
                        help="data table to iterate over", metavar="TABLE")
  parser.add_option("-y", "--history", dest="history", default='history',
                        help="name of table to story iteratoin history (default history)", metavar="HIST")
  parser.add_option("-n", "--noact", action="store_true", dest="noact", default=False,
                                          help="just print queries, don't execute over impala")
  parser.add_option("-s", "--step", dest="step", default=0.1, type="float",
                                          help="step size for SGD (default 0.1)")
  parser.add_option("-d", "--decay", dest="decay", default=0.95, type="float",
                                          help="step size decay (default 0.95)")
  parser.add_option("-u", "--mu", dest="mu", default=0, type="float",
                                          help="regularizer weight (defualt 0)")
  parser.add_option("-e", "--epochs", dest="epochs", default=1, type="int",
                                          help="number of epochs to run (default 1)")

  (options, args) = parser.parse_args()

  # we need at least 1 label and 1 feature to train
  if len(args) < 2:
    parser.print_usage()
    return

  if options.database is None:
    print 'use --db to specify a database to use.'
    return
  if options.table is None:
    print 'use --table to specify the data table.'
    return

  # list of queries will submit to impala
  qry = []

  mod_table = options.history
  dat_table = options.table
  step = options.step
  mu = options.mu
  label = args[0]

  # the query which creates an array out of the feature columns
  arr = 'toarray(%s)' % (', '.join(map(lambda f: '%s.%s' % (dat_table, f), args[1:])))

  qry.append(iutil.make_model_table(mod_table))

  # create all the queries
  for i in xrange(1, options.epochs+1):
    qry.append(logr_epoch(mod_table, dat_table, label, arr, i, step=step, mu=mu))
    step = step * options.decay

  # submit a query to to compute the loss
  # qry.append(logr_loss(mod_table, dat_table, label, arr, epoch=options.epochs))

  # print the query for reference
  for q in qry:
    print q

  # submit the queries to impala
  if not options.noact:
    iutil.impala_shell_exec(qry, database=options.database)

def logr_epoch(model_table, dat_table, label, arr, epoch, step=0.1, mu=0.1):
  ''' Generates a query to train the model
  This will create a query to insert into the state table.
  @param model_table: name of table to store models
  @param dat_table: name of table we iterate over
  @param label: name of label column
  @param arr: UDF that returns an double array stored as a string
  @param epoch: the epoch number (first epoch is 1, the 0th epoch is an empty string)
  '''
  return iutil.bismarck_epoch(model_table, dat_table, 'logr(__PREV_MODEL__, %(arr)s, '
      '%(label)s, %(step)s, %(mu)s)' % {'arr':arr, 'label':label, 'step':step,
        'mu':mu}, epoch, label)

def logr_loss(model_table, dat_table, label, arr, epoch):
  ''' Computes the loss for each example in the data table
  @param model_table: name of table to store models
  @param dat_table: name of table we iterate over
  @param label: name of label column
  @param arr: UDF that returns an double array stored as a string
  @param epoch: the epoch number (which model # to use when computing the loss)
  '''
  return iutil.bismarck_query('logrloss(__PREV_MODEL__, %(arr)s, %(label)s)' %
      {'arr':arr, 'label':label}, model_table, dat_table, epoch, label)

if __name__ == '__main__':
  main()
