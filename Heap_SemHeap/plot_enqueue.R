# Grafico de Insercao - Escala Linear
dados <- read.csv(file.choose())

plot(
  dados$Tamanho,
  dados$Insercao_Sem_Heap,
  type = "b",
  col = "red",
  pch = 19,
  ylim = range(dados[, 2:3]),
  xlab = "Tamanho da fila",
  ylab = "Número de comparações",
  main = "Inserção: Sem Heap vs Com Heap"
)

lines(
  dados$Tamanho,
  dados$Insercao_Com_Heap,
  type = "b",
  col = "blue",
  pch = 19
)

legend(
  "topleft",
  legend = c("Inserção sem Heap", "Inserção com Heap"),
  col = c("red", "blue"),
  pch = 19,
  lty = 1
)
