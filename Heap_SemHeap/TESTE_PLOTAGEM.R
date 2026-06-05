#OBS para valores proximos de Y conflita e fica um em cima do outro
dados <- read.csv(file.choose())
plot(
  dados$Tamanho,
  dados$Insercao_Sem_Heap,
  type = "b",
  col = "red",
  pch = 19,
  ylim = range(dados[, 2:5]),
  xlab = "Tamanho da fila",
  ylab = "Número de comparações",
  main = "Comparações: Fila de Prioridade sem Heap vs com Heap"
)

lines(
  dados$Tamanho,
  dados$Insercao_Com_Heap,
  type = "b",
  col = "blue",
  pch = 19
)

lines(
  dados$Tamanho,
  dados$Remocao_Sem_Heap,
  type = "b",
  col = "darkgreen",
  pch = 19
)

lines(
  dados$Tamanho,
  dados$Remocao_Com_Heap,
  type = "b",
  col = "purple",
  pch = 19
)

legend(
  "topleft",
  legend = c(
    "Inserção sem Heap",
    "Inserção com Heap",
    "Remoção sem Heap",
    "Remoção com Heap"
  ),
  col = c("red", "blue", "darkgreen", "purple"),
  pch = 19,
  lty = 1
)