""" Python module for reading distributions.
"""

#-------------------------------------------------------------------------------

import numpy as np
import collections # for sorting of dictionary
  
#-------------------------------------------------------------------------------
#=== Storage classes ===========================================================
# Storage classes that can be used by user to further analyse distribution
#-------------------------------------------------------------------------------

class Bin:
  """ Storage class for a distribution bin.
  """
  def __init__(self):
    self.centers = []
    self.value = []

#-------------------------------------------------------------------------------
  
class Distribution:
  """ A distribution of arbitrary dimensionality.
  """
  def __init__(self):
    self.name = ""
    self.pol_config = ""
    self.dim = ""
    self.n_bins = 0
    self.bins = []
    
  def projection(self, axis, min_value=0):
    """ Get the projection along the given axis.
        Can include a cut on the individual bin value that is being projected.
    """
    if (axis > self.dim-1):
      raise ValueError("Distribution: axis out of range!", axis, dim)
    sum_dict = {}
    # Sum up bin values if they fulfill condition and make sure each bin gets
    # a value of at least 0
    for bin in self.bins:
      if (bin.value > min_value):
        if bin.centers[axis] in sum_dict.keys():
          sum_dict[bin.centers[axis]] += bin.value
        else:
          sum_dict[bin.centers[axis]] = bin.value
      elif not (bin.centers[axis] in sum_dict.keys()):
        sum_dict[bin.centers[axis]] = 0
        
    # Create sorted bins
    x_values = []
    y_values = []
    for x,y in collections.OrderedDict(sorted(sum_dict.items())).items():
      x_values.append(x)
      y_values.append(y)
      
    return x_values, y_values
    
  def integral(self, min_value=0):
    """ Get the distribution integral.
        Can take a cut-off value that a bin must pass to be counted.
    """
    sum = 0
    for bin in self.bins:
      if (bin.value > min_value):
        sum += bin.value
    return sum

#-------------------------------------------------------------------------------
#===============================================================================
#-------------------------------------------------------------------------------

class Markers:
  """ Define markers.
  """
  distr_beg = "<=====DISTR-BEGIN=====>"
  distr_end = "<=====DISTR-END=======>"
    
#-------------------------------------------------------------------------------

def read_distr_lines(lines):
    """ Read the lines defining a distribution and return the corresponding 
        distribution class object.
    """
    distr = Distribution()

    for l in range(len(lines)):
      split_line = lines[l].split(" ")
      if len(split_line) == 0 : continue # Ignore empty lines
      
      if (split_line[0] == "Name:"):
        # Found distribution name
        distr.name = split_line[1]
      if (split_line[0] == "PolConfig:"):
        # Found polarisation configuration
        distr.pol_config = split_line[1]
      if (split_line[0] == "NBins:"):
        # Found number of bins
        distr.n_bins = int(split_line[1])
      if (split_line[0] == "Dim:"):
        # Found dimensionality of bins
        distr.dim = int(split_line[1])
      if (split_line[0] == "Bin-ID"):
        # Found beginning of actual distribution, start extraction
        for b in range(l+1,l+1+distr.n_bins):
          split_line = lines[b].split(" ")
          bin = Bin()
          for c in range(1, distr.dim+1):
              bin.centers.append(float(split_line[c]))
          bin.value = float(split_line[distr.dim+1])
          distr.bins.append(bin)
        break # Nothing should be after this, done!
    
    return distr

#-------------------------------------------------------------------------------

class DistrReader:
  """ Class for reading the output file produced by the binary executable.
      Will read the distributions from that file that can then be found in the
      `distrs` member variable.
  """
  markers = Markers()
  
  def __init__(self,file_path):
    self.file_path = file_path
    self.lines = [] 
    self.distrs = [] 
    
  def identify_distrs(self):
    """ Split the input lines into the lines from the individual distributions.
    """
    distrs = []
    current_distr = []
    found_distr = False
    for line in self.lines:
      line = line.strip() # Remove trailing/leading whitespaces etc.
      if line == self.markers.distr_beg:
        # Found beginning of distr -> Start reading lines
        found_distr = True
      elif (line == self.markers.distr_end) and (current_distr != []):
        # Found end of one distr, don't read lines unless new one found
        if ( found_distr ):
          distrs.append(current_distr)
        current_distr = []
        found_distr = False
      elif found_distr:
        # Append line to current distr
        current_distr.append(line)
    return distrs

  def read(self):
    """ Read and interpret the line of the input file.
    """
    with open(self.file_path) as f:
      self.lines = f.readlines() # list containing lines of file

    distr_lines = self.identify_distrs()
    for distr_line in distr_lines:
      self.distrs.append(read_distr_lines(distr_line))

#-------------------------------------------------------------------------------